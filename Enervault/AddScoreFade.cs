using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using DG.Tweening;

public class AddScoreFade : MonoBehaviour
{
	[Tooltip("+Image")]
	[SerializeField] Image plusImage;
	[Tooltip("各桁のImage")]
	[SerializeField] Image[] addScoreImages;

	private RectTransform _rect;
	private RectTransform[] _rectAddScores;
	private int[] _addDigits;

	private Vector3 _fadeStartPos;
	private Vector3 _fadeEndPos;
	private Vector3 _digitStartPos;
	private float _digitHeight = 63f;
	private float _digitWidth = 54f;

	private CountUpScoreManager _countUpManager;

	// Start is called once before the first execution of Update after the MonoBehaviour is created
	void Awake()
	{
		_fadeStartPos = transform.localPosition;
		_fadeEndPos = new Vector3(_fadeStartPos.x, 0f, 0f);

		int len = addScoreImages.Length;
		_addDigits = new int[len];
		_rectAddScores = new RectTransform[len];

		for (int i = 0; i < len; i++)
		{
			_addDigits[i] = 0;
			_rectAddScores[i] = addScoreImages[i].rectTransform;
			addScoreImages[i].color = new Color(1f, 1f, 1f, 0f);
		}

		_digitStartPos = _rectAddScores[0].transform.localPosition;

		_rect = GetComponent<RectTransform>();
	}

	/// <summary>アニメーションの設定</summary>
	public void AnimSet(int val, float duration, float delay)
	{
		int len = addScoreImages.Length;
		int digitVal = 0;

		for (int i = 0; val > 0; i++)
		{
			int digit = val % 10;
			_addDigits[i] = digit;

			val /= 10;
			digitVal++;
		}

		plusImage.transform.localPosition += new Vector3((len - digitVal) * _digitWidth, 0f, 0f);
		for (int i = 0; i < digitVal; i++)
		{
			float targetY = _digitHeight * _addDigits[i] + _digitStartPos.y;
			_rectAddScores[i].localPosition = new Vector3(_rectAddScores[i].localPosition.x, targetY, 0f);
			addScoreImages[i].color = new Color(1f, 1f, 1f, 1f);
		}

		StartCoroutine(AddScoreAnim(digitVal, duration, delay));
	}

	/// <summary>加算スコアのアニメーション</summary>
	IEnumerator AddScoreAnim(int digitVal, float duration, float delay)
	{
		yield return new WaitForSeconds(delay);
		_countUpManager.CountUpAnim();

		_rect.DOAnchorPosY(_fadeEndPos.y, duration).
			OnComplete(() => Destroy(gameObject));

		int len = addScoreImages.Length;
		Color[] colors = new Color[len];

		float startAlpha = 1f;
		float time = 0f;

		for (int i = 0; i < len; i++)
		{
			colors[i] = addScoreImages[i].color;
		}

		while (time < duration)
		{
			float alpha = Mathf.Lerp(startAlpha, 0f, time / duration);
			for (int i = 0; i < digitVal; i++)
			{
				colors[i] = new Color(colors[i].r, colors[i].g, colors[i].b, alpha);
				addScoreImages[i].color = colors[i];
			}
			plusImage.color = new Color(plusImage.color.r, plusImage.color.g, plusImage.color.b, alpha);

			time += Time.deltaTime;
			yield return null;
		}
	}

	public void SetCountUpManager(CountUpScoreManager manager)
	{
		_countUpManager = manager;
	}
}

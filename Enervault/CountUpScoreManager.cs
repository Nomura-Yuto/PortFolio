using UnityEngine;
using UnityEngine.UI;
using DG.Tweening;



public class CountUpScoreManager : MonoBehaviour
{
	[System.Serializable]
	public class Digit
	{
		public int val = 0;
		public GameObject gameObject;
		[HideInInspector] public Image image;
		[HideInInspector] public RectTransform rectTransform;

		public void Init()
		{
			val = 0;
			if (gameObject == null) return;

			image = gameObject.GetComponent<Image>();
			if (image != null)
			{
				image.color = new Color(1f, 1f, 1f, 0.2f);
			}
			rectTransform = gameObject.GetComponent<RectTransform>();
		}
	};

	[Header("ScoreSetting")]
	[SerializeField] int score = 0;
	[SerializeField] int maxDigit = 7;
	[SerializeField] float animTime = 0.5f;
	[Tooltip("桁の設定")]
	[SerializeField] Digit[] digits;

	[Header("AddScoreSetting")]
	[Tooltip("消えるまでの時間")]
	[SerializeField] float fadeDuration = 0.5f;
	[Tooltip("動き始めるまでの時間")]
	[SerializeField] float fadeDelay = 0.1f;
	[Tooltip("加算スコアオブジェクト")]
	[SerializeField] GameObject addScoreObj;
	[Tooltip("描画順の調節用")]
	[SerializeField] Transform childScoreMask;

	private float _digitStartY = -330;
	private float _digitEndY = 300;
	private float _digitHeight = 63f;

	private CountUpScoreManager _manager;

	// Start is called once before the first execution of Update after the MonoBehaviour is created
	void Start()
	{
		for (int i = 0; i < maxDigit; i++)
		{
			digits[i].Init();
		}

		_manager = GetComponent<CountUpScoreManager>();
	}

	private void Update()
	{

	}

	/// <summary>スコアの加算</summary>
	public void AddScore(int value)
	{
		score += value;

		Vector3 initPos = new Vector3(0f, -80f, 0f);
		GameObject obj = Instantiate(addScoreObj, new Vector3(0f, 0f, 0f), Quaternion.identity, transform);
		childScoreMask.SetAsLastSibling();
		obj.GetComponent<RectTransform>().localPosition = initPos;

		AddScoreFade fade = obj.GetComponent<AddScoreFade>();
		fade.SetCountUpManager(_manager);
		fade.AnimSet(value, fadeDuration, fadeDelay);
	}


	/// <summary>スコア加算アニメーション</summary>
	public void CountUpAnim()
	{
		GetScoreDigits(score);

		DigitAnim(0);

		for(int i = 1; i <= maxDigit; i++)
		{
			if (score < Mathf.Pow(10, i)) return;
			DigitAnim(i);
		}
	}


	/// <summary>各桁のアニメーション</summary>
	void DigitAnim(int num)
	{
		RectTransform rect = digits[num].rectTransform;

		// 再生中のアニメーションを中断
		rect.DOKill();

		digits[num].image.color = new Color(1f, 1f, 1f, 1f);

		float targetY = _digitHeight * digits[num].val + _digitStartY;

		if (digits[num].val == 0)
		{
			// 前回と同じ場合スキップ
			if (rect.localPosition.y != targetY)
			{
				// 一番下の0まで再生した後、初期座標にリセット
				rect.DOAnchorPosY(_digitEndY, animTime).
					 OnComplete(() => {
						 rect.localPosition = new Vector3(rect.localPosition.x, _digitStartY, 0f);
					 });
			}
		}
		else if (rect.localPosition.y > targetY)
		{
			// 移動量と時間の計算
			float firstHalf = _digitEndY - rect.localPosition.y;
			int step = (int)(firstHalf + targetY - _digitStartY);
			float ratio = animTime / step;
			float time = ratio * (int)firstHalf;

			// 数字が下がっていかないように対処
			rect.DOAnchorPosY(_digitEndY, time).SetEase(Ease.Linear).
				OnComplete(() => {
					rect.localPosition = new Vector3(rect.localPosition.x, _digitStartY, 0f);
					rect.DOAnchorPosY(targetY, animTime - time).SetEase(Ease.Linear);
				});
		}
		else
		{
			rect.DOAnchorPosY(targetY, animTime);
		}
	}

	/// <summary>各桁の取得</summary>
	private void GetScoreDigits(int num)
	{
		if (num >= Mathf.Pow(10, maxDigit))
		{
			for (int i = 0; i < maxDigit; i++)
			{
				digits[i].val = 9;
			}
		}
		else
		{
			for (int i = 0; num > 0; i++)
			{
				int digit = num % 10;

				digits[i].val = digit;

				num /= 10;
			}
		}
	}
}

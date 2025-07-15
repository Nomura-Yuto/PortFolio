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
	[Tooltip("���̐ݒ�")]
	[SerializeField] Digit[] digits;

	[Header("AddScoreSetting")]
	[Tooltip("������܂ł̎���")]
	[SerializeField] float fadeDuration = 0.5f;
	[Tooltip("�����n�߂�܂ł̎���")]
	[SerializeField] float fadeDelay = 0.1f;
	[Tooltip("���Z�X�R�A�I�u�W�F�N�g")]
	[SerializeField] GameObject addScoreObj;
	[Tooltip("�`�揇�̒��ߗp")]
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

	/// <summary>�X�R�A�̉��Z</summary>
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


	/// <summary>�X�R�A���Z�A�j���[�V����</summary>
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


	/// <summary>�e���̃A�j���[�V����</summary>
	void DigitAnim(int num)
	{
		RectTransform rect = digits[num].rectTransform;

		// �Đ����̃A�j���[�V�����𒆒f
		rect.DOKill();

		digits[num].image.color = new Color(1f, 1f, 1f, 1f);

		float targetY = _digitHeight * digits[num].val + _digitStartY;

		if (digits[num].val == 0)
		{
			// �O��Ɠ����ꍇ�X�L�b�v
			if (rect.localPosition.y != targetY)
			{
				// ��ԉ���0�܂ōĐ�������A�������W�Ƀ��Z�b�g
				rect.DOAnchorPosY(_digitEndY, animTime).
					 OnComplete(() => {
						 rect.localPosition = new Vector3(rect.localPosition.x, _digitStartY, 0f);
					 });
			}
		}
		else if (rect.localPosition.y > targetY)
		{
			// �ړ��ʂƎ��Ԃ̌v�Z
			float firstHalf = _digitEndY - rect.localPosition.y;
			int step = (int)(firstHalf + targetY - _digitStartY);
			float ratio = animTime / step;
			float time = ratio * (int)firstHalf;

			// �������������Ă����Ȃ��悤�ɑΏ�
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

	/// <summary>�e���̎擾</summary>
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

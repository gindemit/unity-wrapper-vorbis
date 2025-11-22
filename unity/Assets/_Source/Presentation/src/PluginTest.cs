using System.IO;
using UnityEngine;
using UnityEngine.UI;
using TMPro;
using OggVorbis;

public class PluginTest : MonoBehaviour
{
    [Header("External references")]
    [SerializeField] private AudioSource _sourceAudio;
    [SerializeField] private AudioSource _loadedAudio;
    [Header("Internal references")]
    [SerializeField] private TMP_InputField _fileNameInputField;
    [SerializeField] private TextMeshProUGUI _finalFilePathOggText;
    [SerializeField] private TextMeshProUGUI _sourceAudioStats;
    [SerializeField] private TextMeshProUGUI _loadedAudioStats;
    [SerializeField] private TextMeshProUGUI _tookText;
    [SerializeField] private Button _saveOggButton;
    [SerializeField] private Button _loadOggButton;
    [SerializeField] private Slider _baseQualitySlider;
    [SerializeField] private TextMeshProUGUI _baseQualityValueText;
    [SerializeField] private TextMeshProUGUI _fileSizeValueText;

    [SerializeField] private Button _playPauseSourceButton;
    [SerializeField] private TextMeshProUGUI _playPauseSourceButtonText;
    [SerializeField] private Button _playPauseLoadedButton;
    [SerializeField] private TextMeshProUGUI _playPauseLoadedButtonText;

    [SerializeField] private int _samplesToRead = 2048;

    private System.Diagnostics.Stopwatch _stopwatch;
    private string _workingDirectory;

    private void Awake()
    {
        if (_sourceAudio.clip == null)
        {
            Debug.LogError("Please provide a clip to source audio");
            return;
        }
        _stopwatch = new System.Diagnostics.Stopwatch();
        _workingDirectory = Path.Combine(Application.persistentDataPath, "VorbisPluginTest");
        Debug.Log(_workingDirectory);
        Directory.CreateDirectory(_workingDirectory);
        _fileNameInputField.text = _sourceAudio.clip.name;
        OnBaseQualitySliderValueChanged(_baseQualitySlider.value);

        UpdateFinalPaths();
        UpdateSourceAudioStats();
        UpdateLoadedAudioStats(string.Empty);
    }

    private void OnEnable()
    {
        _fileNameInputField.onValueChanged.AddListener(OnFileNameInputFieldValueChanged);
        _saveOggButton.onClick.AddListener(OnSaveOggButtonClick);
        _loadOggButton.onClick.AddListener(OnLoadOggButtonClick);
        _playPauseSourceButton.onClick.AddListener(OnPlayPauseSourceButtonClick);
        _playPauseLoadedButton.onClick.AddListener(OnPlayPauseLoadedButtonClick);
        _baseQualitySlider.onValueChanged.AddListener(OnBaseQualitySliderValueChanged);
    }
    private void OnDisable()
    {
        _fileNameInputField.onValueChanged.RemoveListener(OnFileNameInputFieldValueChanged);
        _saveOggButton.onClick.RemoveListener(OnSaveOggButtonClick);
        _loadOggButton.onClick.RemoveListener(OnLoadOggButtonClick);
        _playPauseSourceButton.onClick.RemoveListener(OnPlayPauseSourceButtonClick);
        _playPauseLoadedButton.onClick.RemoveListener(OnPlayPauseLoadedButtonClick);
        _baseQualitySlider.onValueChanged.RemoveListener(OnBaseQualitySliderValueChanged);
    }

    private void OnFileNameInputFieldValueChanged(string value)
    {
        UpdateFinalPaths();
    }
    private void OnSaveOggButtonClick()
    {
        _stopwatch.Restart();
        VorbisPlugin.Save(_finalFilePathOggText.text, _sourceAudio.clip, _baseQualitySlider.value, _samplesToRead);
        _tookText.text = _stopwatch.ElapsedMilliseconds.ToString();
        FileInfo fileInfo = new FileInfo(_finalFilePathOggText.text);
        long fileSize = fileInfo.Length;
        _fileSizeValueText.text = Support.ToStringHelpers.FIleSizeFormatter.ToPrettyFileSize(fileSize);
        Debug.Log($"Vorbis ogg file save took {_stopwatch.ElapsedMilliseconds} ms.");
    }
    private void OnLoadOggButtonClick()
    {
        _stopwatch.Restart();
        _loadedAudio.clip = VorbisPlugin.Load(_finalFilePathOggText.text, _samplesToRead);
        _tookText.text = _stopwatch.ElapsedMilliseconds.ToString();
        Debug.Log($"Load vorbis ogg file took {_stopwatch.ElapsedMilliseconds} ms.");
        UpdateLoadedAudioStats("ogg");
    }
    private void OnPlayPauseSourceButtonClick()
    {
        PlayPauseAudioSource(_sourceAudio, _playPauseSourceButtonText, "Source Audio");
    }
    private void OnPlayPauseLoadedButtonClick()
    {
        PlayPauseAudioSource(_loadedAudio, _playPauseLoadedButtonText, "Loaded Audio");
    }

    private static void PlayPauseAudioSource(AudioSource audioSource, TextMeshProUGUI text, string suffix)
    {
        if (audioSource == null)
        {
            return;
        }
        if (audioSource.isPlaying)
        {
            audioSource.Pause();
            text.text = "Play " + suffix;
        }
        else
        {
            audioSource.Play();
            text.text = "Pause " + suffix;
        }
    }
    private void UpdateFinalPaths()
    {
        string fileName = _fileNameInputField.text;
        string pathToSave = Path.Combine(_workingDirectory, fileName);
        _finalFilePathOggText.text = pathToSave + ".ogg";
    }
    private void UpdateSourceAudioStats()
    {
        AudioClip clip = _sourceAudio.clip;
        _sourceAudioStats.text = $"{clip.name}, {clip.length} sec., {clip.frequency} kHz, {clip.channels} ch.";
    }
    private void UpdateLoadedAudioStats(string format)
    {
        AudioClip clip = _loadedAudio.clip;
        if (clip == null)
        {
            _loadedAudioStats.text = "Audio clip is null";
            return;
        }
        _loadedAudioStats.text = $"{clip.name}, {format}, {clip.length} sec., {clip.frequency} kHz, {clip.channels} ch.";
    }
    private void OnBaseQualitySliderValueChanged(float value)
    {
        _baseQualityValueText.text = value.ToString("0.00");
    }
}

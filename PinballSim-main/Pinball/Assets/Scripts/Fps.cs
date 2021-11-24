using UnityEngine.UI;
using UnityEngine;

public class Fps : MonoBehaviour
{

    int fps = 0;

    int secondsSinceStart = 0;
    int MinutesSinceStart = 0;
    int hoursSinceStart = 0;

    float fpsTimer = 0f;
    Text text;

    private void Start() {
        text = GetComponentInChildren<Text>();
    }

    // Update is called once per frame
    void Update() {
        fps++;
        fpsTimer += Time.deltaTime;

        if (fpsTimer > 1) {
            secondsSinceStart++;

            if (secondsSinceStart == 60) {
                secondsSinceStart = 0;
                MinutesSinceStart++;
            }

            if (MinutesSinceStart == 60) {
                MinutesSinceStart = 0;
                hoursSinceStart++;
            }

            text.text = "Fps: " + fps + "\n" + "Sss: " + secondsSinceStart + "\n" + "mss: " + MinutesSinceStart + "\n" + "Hss: " + hoursSinceStart;

            fpsTimer -= 1f;

            fps = 0;

        }
    }
}

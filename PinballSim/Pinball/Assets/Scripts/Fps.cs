/*
 * This script will keep track of the fps and time sisnce start in secodns minutes and hours.
 * It will then display these values in the text component that this script is attached to.
 */

using UnityEngine.UI;
using UnityEngine;

public class Fps : MonoBehaviour
{

    int fps = 0;

    // ints for storing the time values. Used for seeing how long the trainer runs.
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

        // Every frame we increment the timer, by the elapsed time since the last frame. Reported by Time.deltaTime.
        fpsTimer += Time.deltaTime;

        if (fpsTimer > 1f) {
            // Increment the second count by 1 every second.
            secondsSinceStart++;

            // Increment the minute count by 1 if 60 seconds have elapsed.
            if (secondsSinceStart >= 60) {
                secondsSinceStart -= 60;
                MinutesSinceStart++;
            }

            // Increment the hour count by 1 if 60 seconds have elapsed.
            if (MinutesSinceStart >= 60) {
                MinutesSinceStart -= 60;
                hoursSinceStart++;
            }

            // Display the counters.
            text.text = "Fps: " + fps + "\n" + "Sss: " + secondsSinceStart + "\n" + "mss: " + MinutesSinceStart + "\n" + "Hss: " + hoursSinceStart;

            // Decremept the fpsTimer by 1f. We cant set it to 0 because it is probably a bit mroe than 1, eg 1.078f.
            fpsTimer -= 1f;

            fps = 0;
        }
    }
}

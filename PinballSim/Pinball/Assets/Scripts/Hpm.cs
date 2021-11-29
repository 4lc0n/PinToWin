/*
 * This script keeps track of how many hits per minute the agent scores.
 * And also the highes hit streak in 1 minute.
 */

using UnityEngine.UI;
using UnityEngine;

public class Hpm : MonoBehaviour
{

    public GameObject goal1;
    public GameObject goal2;

    int hitsPerMinute = 0;
    int highestHitsPerMinute = 0;

    float hitTimer = 0f;
    Text text;

    private void Start() {
        text = GetComponentInChildren<Text>();

        // Subscribe to the HitGoal callback events.
        goal1.GetComponent<GoalController>().RegisterGoalHit(goalHitTrigger);
        goal2.GetComponent<GoalController>().RegisterGoalHit(goalHitTrigger);
    }

    // Update is called once per frame
    void Update() {
        hitTimer += Time.deltaTime;

        // Check if 1 minute has elapsed.
        if (hitTimer > 60f) {

            if (hitsPerMinute > highestHitsPerMinute) {
                // Update the highestHitsPerMinute with the new streak if we beat it.

                highestHitsPerMinute = hitsPerMinute;
            }

            // Display the stats.
            text.text = "Hpm: " + hitsPerMinute + "\n" + "Hhpm: " + highestHitsPerMinute;

            // Reset the counters.
            hitsPerMinute = 0;
            hitTimer -= 60f;
        }
    }

    // Function that gets called when the ball hits a goal.
    void goalHitTrigger(GoalController goalController) {
        hitsPerMinute++;
    }
}

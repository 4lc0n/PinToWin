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
        goal1.GetComponent<GoalController>().RegisterGoalHit(goalHitTrigger);
        goal2.GetComponent<GoalController>().RegisterGoalHit(goalHitTrigger);
    }

    // Update is called once per frame
    void Update() {
        hitTimer += Time.deltaTime;

        if (hitTimer > 60) {

            if (hitsPerMinute > highestHitsPerMinute) {
                highestHitsPerMinute = hitsPerMinute;
            }

            text.text = "Hpm: " + hitsPerMinute + "\n" + "Hhpm: " + highestHitsPerMinute;

            hitsPerMinute = 0;
            hitTimer = 0f;
        }
    }

    void goalHitTrigger(GoalController goalController) {
        hitsPerMinute++;
    }
}

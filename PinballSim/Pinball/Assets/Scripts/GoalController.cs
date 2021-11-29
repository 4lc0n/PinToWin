/*
 * This script controlls the goals.
 */

using System;
using UnityEngine;

public class GoalController : MonoBehaviour
{
    public GameObject ball;

    // Unity fucntion thats gets called whenever a object collides with the gameobjects capsule collider.
    void OnCollisionEnter(Collision _other) {
        if (_other.gameObject == ball) {
            // Check if the collede object is the ball. And if so trigger the callback.
            cbGoalHit?.Invoke(this);
        }
    }

    Action<GoalController> cbGoalHit;

    // Function to subscribe to the callback 
    public void RegisterGoalHit(Action<GoalController> callbackfunc) {
        cbGoalHit += callbackfunc;
    }
}

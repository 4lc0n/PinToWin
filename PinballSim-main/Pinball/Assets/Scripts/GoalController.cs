using System.Collections;
using System.Collections.Generic;
using System;
using UnityEngine;

public class GoalController : MonoBehaviour
{

    public GameObject ball;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
    }

    void OnCollisionEnter(Collision _other) {
        if (_other.gameObject == ball) {
            cbGoalHit?.Invoke(this);
        }
    }

    Action<GoalController> cbGoalHit;

    public void RegisterGoalHit(Action<GoalController> callbackfunc) {
        cbGoalHit += callbackfunc;
    }



}

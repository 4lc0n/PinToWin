using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using Unity.MLAgents;
using Unity.MLAgents.Sensors;
using Unity.MLAgents.Actuators;

public class MasterAgent : Agent
{
    public GameObject ball;

    public GameObject leftFlipper;
    public GameObject rigthFlipper;

    public GameObject goal1;
    public GameObject goal2;

    // Start is called before the first frame update
    void Start() {
        goal1.GetComponent<GoalController>().RegisterGoalHit(goalHitTrigger);
        goal2.GetComponent<GoalController>().RegisterGoalHit(goalHitTrigger);
    }


    public override void OnEpisodeBegin() {

    }

    public override void CollectObservations(VectorSensor sensor) {
        sensor.AddObservation(ball.transform);
        sensor.AddObservation(rigthFlipper.GetComponent<HingeJoint>().angle);

    }

    public override void OnActionReceived(ActionBuffers actions) {
        if (actions.ContinuousActions[0] > 0.5f) {
            rigthFlipper.GetComponent<PaddleController>().activate();
        }

        if (actions.ContinuousActions[1] > 0.5f) {
            leftFlipper.GetComponent<PaddleController>().activate();
        }
    }

    public override void Heuristic(in ActionBuffers actionsOut) {
        ActionSegment<float> continuousActionsOut = actionsOut.ContinuousActions;

        if (Input.GetKey("t")) {
            continuousActionsOut[0] = 1f;
        }

        if (Input.GetKey("y")) {
            continuousActionsOut[1] = 1f;
        }
    }


    // The ball falls out.
    void outOfBoundsTrigger(OutOfBounds outOfBounds) {
        SetReward(-10f);
        EndEpisode();
    }

    void goalHitTrigger(GoalController goalController) {
        SetReward(1f);
        Debug.Log("GoalHit");
    }
}



// mlagents-learn config/pinball_config.yaml --run-id=Pinball --force
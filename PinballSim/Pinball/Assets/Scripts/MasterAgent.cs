/*
 * This script control the Unity.ML agent that controls the machine learning part of this simulation.
 * This script is attached to a custom Unity.ML gameobject.
 * 
 * To run the trainer, open powershell and go to the install location of Unity.ML.
 * cd C:\Users\ insert path here \ml-agents-release_2
 * 
 * Then run this command.
 * mlagents-learn config/pinball_config.yaml --run-id=Pinball --force
 * 
 * It is importantt hat the config file at the above location has been added adn that the names are correct.
 * 
 * Documentation: https://github.com/Unity-Technologies/ml-agents/blob/release_18_docs/docs/Readme.md
 */

using Unity.MLAgents;
using Unity.MLAgents.Actuators;
using Unity.MLAgents.Sensors;
using UnityEngine;

// This class exetnds the Agent interface.
public class MasterAgent : Agent
{

    // Set up all the objects that this script needs to acces or keep track of.
    // Can probably be improved.
    public GameObject ball;

    public GameObject leftFlipper;
    public GameObject rigthFlipper;

    public GameObject goal1;
    public GameObject goal2;

    // This vector will be updated with the position of any sensor that detects the ball.
    // Currently not implemented.
    Vector3 ballPositionFromSensors;

    // Start is called before the first frame update
    void Start() {
        // Subscribe to the HitGoal callback events.
        goal1.GetComponent<GoalController>().RegisterGoalHit(goalHitTrigger);
        goal2.GetComponent<GoalController>().RegisterGoalHit(goalHitTrigger);

        // Uncomment this line to subscribe to the OfBoundsTrigger event.
        // OutOfBounds.outOfBounds.RegisterOutOfBounds(outOfBoundsTrigger);

        ballPositionFromSensors = Vector3.zero;
    }

    // Required part of the Agent interface.
    public override void OnEpisodeBegin() {
        // All episode startup code should be placed here. For now we dont have any.
    }

    // Required part of the Agent interface.
    public override void CollectObservations(VectorSensor sensor) {
        // Here we set up all the values that we want Unity.Ml to see. Namely the position of the ball and the angle of the rigth flipper. Not sure if the angle is needed.

        sensor.AddObservation(ball.transform);
        sensor.AddObservation(rigthFlipper.GetComponent<HingeJoint>().angle);
    }

    // Required part of the Agent interface.
    public override void OnActionReceived(ActionBuffers actions) {
        // Here we set up the actions that the agent should do. Namely call the flippers activate functions.

        if (actions.ContinuousActions[0] > 0.5f) {
            rigthFlipper.GetComponent<PaddleController>().activate();
        }

        if (actions.ContinuousActions[1] > 0.5f) {
            leftFlipper.GetComponent<PaddleController>().activate();
        }
    }

    // Optional part of the Agent interface.
    public override void Heuristic(in ActionBuffers actionsOut) {
        // This function is used to manually control the Agent for testing purposes.
        // To enable this functionality, we need to set Behavior type to heuristics only in the gameobjects behavior parameters.
        ActionSegment<float> continuousActionsOut = actionsOut.ContinuousActions;

        // This will set the values of the first and secon action to 1, wich will trigger them.
        if (Input.GetKey("t")) {
            continuousActionsOut[0] = 1f;
        }

        if (Input.GetKey("y")) {
            continuousActionsOut[1] = 1f;
        }
    }


    // The ball falls out.
    // Just realized that this callback is never subscribed to so it doesn't actually do anything.
    // Fixing this might fix the issue with the Agent getting stuck spammign flipper.activate.
    // I have added the code that will do this and commented it out.
    void outOfBoundsTrigger(OutOfBounds outOfBounds) {
        SetReward(-10f);
        EndEpisode();
    }

    // Function that gets callen whenever 1 of the goal is hit by the ball.
    void goalHitTrigger(GoalController goalController) {
        SetReward(1f);
        Debug.Log("GoalHit");
    }
}

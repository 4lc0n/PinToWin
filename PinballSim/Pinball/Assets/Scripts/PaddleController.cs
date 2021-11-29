/*
 * This script controlls the flippers.
 */

using System;
using UnityEngine;

public class PaddleController : MonoBehaviour
{
    public float targetPosition = 0F;
    public string inputButtonName = "";

    private new HingeJoint hingeJoint;
    JointSpring JointSpring;

    // When triggerFlipper gets called, we set this variable to the length the flipper should be activated by.
    float triggertime = 0f;

    // The activate fucntion can take in a delay before the flipper activates. This delay is stored here.
    float waitTime = 0f;

    // If a delay has been given, this boolean will be set to true.
    bool wait;

    // Start is called before the first frame update
    void Start() {
        // We start by setting up the parameters of the hingejoint.
        hingeJoint = GetComponent<HingeJoint>();

        JointSpring = new JointSpring();

        hingeJoint.useSpring = true;

        JointLimits limits = hingeJoint.limits;
        limits.min = 0;
        limits.bounciness = 0;
        limits.bounceMinVelocity = 0;
        limits.max = targetPosition;
        hingeJoint.useLimits = true;
        hingeJoint.limits = limits;

        JointSpring.spring = 1000000;
        JointSpring.damper = 0;
    }

    // Update is called once per frame
    void Update() {

        // Check if the trigger delay has elapsed or if the manual input key has benn pressed.
        if ((wait && waitTime <= 0f) || Input.GetKey(inputButtonName)) {
            // If so trigger the flipper.
            triggerFlipper();
            wait = false;
        }

        // If not decrement the waitTime with Time.deltaTime.
        else {
            waitTime -= Time.deltaTime;
        }

        if (triggertime > 0f) {
            triggertime -= Time.deltaTime;
        }

        // The flipper is finished activating so we reset it.
        else {
            JointSpring.targetPosition = 0f;
            hingeJoint.spring = JointSpring;
        }
    }

    /// <summary>
    /// Function to setup the flipper to activate.
    /// </summary>
    /// <param name="activationDelay">If we want the flipper to activate after a given time delay. We can pass that delay here.</param>
    public void activate(float activationDelay = 0f) {

        // check if the activation delay is 0 adn if so jus ttrigger the flipper.
        if (activationDelay == 0f) {
            triggerFlipper();
            return;
        }

        // else we start the delay timer.
        waitTime = activationDelay;
        wait = true;
    }

    // callback that triggers when the flipper activtaes.
    Action<PaddleController> cbPaddleActivated;

    public void RegisterPaddleActivated(Action<PaddleController> callbackfunc) {
        cbPaddleActivated += callbackfunc;
    }

    /// <summary>
    /// function to actually activate the flipper.
    /// </summary>
    void triggerFlipper() {
        // Set the time the flipper should be activated by to 0.1f
        triggertime = 0.1f;

        // Set the target of the flippes jointspring to the target position.
        JointSpring.targetPosition = targetPosition;
        hingeJoint.spring = JointSpring;
        
        // Call the callback.
        cbPaddleActivated?.Invoke(this);
    }
}

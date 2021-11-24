using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class PaddleController : MonoBehaviour
{
    public float targetPosition = 0F;
    public string inputButtonName = "LeftPaddle";

    private new HingeJoint hingeJoint;
    JointSpring JointSpring;

    float triggertime = 0f;
    float waitTime = 0f;
    bool wait;

    // Start is called before the first frame update
    void Start() {
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

        JointSpring.spring = 1000000000;
        JointSpring.damper = 0;
    }

    // Update is called once per frame
    void Update() {

        if ((wait && waitTime <= 0) || Input.GetKey(inputButtonName)) {
            triggerFlipper();
            wait = false;
        }

        else {
            waitTime -= Time.deltaTime;
        }

        if (triggertime > 0) {
            triggertime -= Time.deltaTime;
        }

        else {
            JointSpring.targetPosition = 0;
            hingeJoint.spring = JointSpring;
        }
    }

    public void activate(float activationDelay = 0) {
        if (activationDelay == 0) {
            triggerFlipper();
            return;
        }
        waitTime = activationDelay;
        wait = true;
    }

    Action<PaddleController> cbPaddleActivated;

    public void RegisterPaddleActivated(Action<PaddleController> callbackfunc) {
        cbPaddleActivated += callbackfunc;
    }

    void triggerFlipper() {
        //Debug.Log("activate");
        triggertime = 0.1f;

        JointSpring.targetPosition = targetPosition;
        hingeJoint.spring = JointSpring;
        
        cbPaddleActivated?.Invoke(this);
    }
}

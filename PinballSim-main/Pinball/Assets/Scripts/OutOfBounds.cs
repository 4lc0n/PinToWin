using System.Collections;
using System.Collections.Generic;
using System;
using UnityEngine;

public class OutOfBounds : MonoBehaviour
{
    public static OutOfBounds outOfBounds;
    public GameObject ball;

    Vector3 startingPos;
    Vector3 ballPos;

    bool secOne = false;

    float timer = 0f;

    private void OnEnable() {
        outOfBounds = this;
        startingPos = ball.transform.position;
    }

    // Update is called once per frame
    void Update() {
        timer += Time.deltaTime;

        if (Input.GetKey("r")) {
            resetBall();
        }

        if (timer > 1f) {
            if (BallStopped()) {
                resetBall();
            }

            timer = 0f;
        }
    }

    void OnCollisionEnter(Collision _other) {
        if (_other.gameObject == ball) {
            resetBall();
        }
    }

    Action<OutOfBounds> cbOutOfBounds;

    public void RegisterOutOfBounds(Action<OutOfBounds> callbackfunc) {
        cbOutOfBounds += callbackfunc;
    }

    void resetBall() {
        ball.GetComponent<Rigidbody>().velocity = Vector3.zero;
        ball.transform.position = startingPos;
        cbOutOfBounds?.Invoke(this);
        //Debug.Log("OutOfBounds");
    }

    bool BallStopped() {
        if (ballPos == Vector3.zero) {
            ballPos = ball.transform.position;
            return false;
        }

        if (MathF.Abs(ballPos.x - ball.transform.position.x) < 1 && MathF.Abs(ballPos.z - ball.transform.position.z) < 1) {
            if (!secOne) {
                secOne = true;
                return false;
            }
            
            ballPos = Vector3.zero;
            return true;
        }

        else {
            ballPos = Vector3.zero;
        }

        return false;
    }
}

/*
 * This script will detect if the ball collides with the collider assigned to the gameobject this script is attached to.
 * If it does we trigger the OutOfBounds callback.
 */

using System;
using UnityEngine;

public class OutOfBounds : MonoBehaviour
{
    // Public static refrence to this script.
    public static OutOfBounds outOfBounds;
    public GameObject ball;

    // Track the starting position of the ball wich is used to reset it.
    Vector3 startingPos;

    // Store the ball position to chech if the ball is stuck.
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

        // Manually reset the ball.
        if (Input.GetKey("r")) {
            resetBall();
        }

        // Check if the ball has stopped, every second.
        if (timer > 1f) {
            if (BallStopped()) {
                resetBall();
            }

            timer = 0f;
        }
    }
    
    /// <summary>
    /// Check if the ball hits the collider of the gamobject that this script is attached to.
    /// </summary>
    /// <param name="_other">The collided object</param>
    void OnCollisionEnter(Collision _other) {
        if (_other.gameObject == ball) {
            // If so reset the ball.
            resetBall();
        }
    }

    Action<OutOfBounds> cbOutOfBounds;
    
    /// <summary>
    /// Function to subscribe to the out of bound event.
    /// </summary>
    /// <param name="callbackfunc"></param>
    public void RegisterOutOfBounds(Action<OutOfBounds> callbackfunc) {
        cbOutOfBounds += callbackfunc;
    }
    
    void resetBall() {
        // Nullify the balls velocity.
        ball.GetComponent<Rigidbody>().velocity = Vector3.zero;
        // Reset the balls position to the starting position.
        ball.transform.position = startingPos;

        // Call the calback.
        cbOutOfBounds?.Invoke(this);
    }
    
    /// <summary>
    /// Calculate if the ball has stopped.
    /// </summary>
    /// <returns> Returns true if the ball has been stuck for more than 2 second and if not thsi returns false.</returns>
    bool BallStopped() {

        // chech if tphe ballpos is "null" wich in this case is the Vector3.zero
        if (ballPos == Vector3.zero) {
            // If it is, set ballPos to the actual position of the ball adn return false.
            ballPos = ball.transform.position;
            return false;
        }

        // Do a check if the balls position has moved less than 1 transform unit.
        /// Note: This is untested since the dimentions of the board war redone and divided by 10. We migth need to change the < 1 to < 0.1.
        if (MathF.Abs(ballPos.x - ball.transform.position.x) < 1 && MathF.Abs(ballPos.z - ball.transform.position.z) < 1) {

            // If this is the first second this is true, we set the secOne boolean to true and return false.
            if (!secOne) {
                secOne = true;
                return false;
            }

            // If this is the second second in a row that the balls position has been within 1 transform position. set the ballPos to "null" in this case Vector3.zero and return true.
            ballPos = Vector3.zero;
            return true;
        }

        else {
            // If we get here the balls position has deviated more than 1 transfor mposition. Adn therfore, we set the ballPos to "null" in this case Vector3.zero.
            ballPos = Vector3.zero;
        }

        return false;
    }
}

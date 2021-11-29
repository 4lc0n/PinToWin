/*
 * This script controlls the spring, that pushes the ball out of its start location.
 */

using UnityEngine;

public class PullSpring : MonoBehaviour {

    // Assign a input button to trigger the spring.
    public string inputButtonName = "Pull";

    public float distance = 50;
    public float speed = 1;
    public float power = 2000;

    public GameObject ball;

    private bool ready = false;
    private bool fire = false;
    private float moveCount = 0;

    // Update is called once per frame
    void Update() {
        if (Input.GetKey(inputButtonName)) {
            //As the button is held down, slowly move the piece
            if (moveCount < distance) {
                transform.Translate(0, 0, -speed * Time.deltaTime);
                moveCount += speed * Time.deltaTime;
                fire = true;
            }
        }

        else if (moveCount > 0) {
            //Shoot the ball
            if (fire && ready) {
                ball.transform.TransformDirection(Vector3.forward * 10);
                ball.GetComponent<Rigidbody>().AddForce(0, 0, moveCount * power);
                fire = false;
                ready = false;
            }

            //Once we have reached the starting position fire off!
            transform.Translate(0, 0, 20 * Time.deltaTime);
            moveCount -= 20 * Time.deltaTime;
        }

        //Just ensure we don't go past the end
        if (moveCount <= 0) {
            fire = false;
            moveCount = 0;
        }
    }

    // Unity fucntion thats gets called whenever a object collides with the gameobjects box collider.
    void OnCollisionEnter(Collision _other) {
        if (_other.gameObject.tag == "Ball") {
            ready = true;
        }
    }
}

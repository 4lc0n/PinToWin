using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PullSpring : MonoBehaviour {

    public string inputButtonName = "Pull";

    public float distance = 50;
    public float speed = 1;
    public float power = 2000;

    public GameObject ball;

    private bool ready = false;
    private bool fire = false;
    private float moveCount = 0;

    // Start is called before the first frame update
    void Start() {

    }

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

    void OnCollisionEnter(Collision _other) {
        if (_other.gameObject.tag == "Ball") {
            ready = true;
        }
    }
}



// FIXME:: spamming space fucks everything.


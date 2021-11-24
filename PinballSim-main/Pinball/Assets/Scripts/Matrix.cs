using System.Collections;
using System.Collections.Generic;
using System;
using UnityEngine;

public class Matrix : MonoBehaviour
{
    public GameObject paddle_GO;
    PaddleController paddle;

    public GameObject outOfBounds_GO;

    LightSensor[,] sensors;
    public Sprite sprite;

    LightSensor lightSensor1;

    float travelTime;

    bool travelTimer;

    // hitCalculated will be set to true when a hit is calculated, and false when the flipper activates. This is for optimazation
    bool hitCalculated;

    // Start is called before the first frame update
    void Start() {
        int size = 10;
        float t = size / 5;
        sensors = new LightSensor[size, size];

        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {

                GameObject gameObject = new GameObject("Sensor_" + x + "_" + y);
                gameObject.transform.SetParent(transform);
                gameObject.transform.position = new Vector3(transform.position.x + (x / t), 0.1f, transform.position.z + (y / t));
                gameObject.transform.rotation = Quaternion.AngleAxis(90, new Vector3(1, 0, 0));

                SpriteRenderer renderer = gameObject.AddComponent<SpriteRenderer>();
                renderer.sprite = sprite;
                renderer.color = Color.green;

                sensors[x, y] = new LightSensor(gameObject);
                sensors[x, y].RegisterSensorChanged(OnSensorChanged);
            }
        }

        paddle = paddle_GO.GetComponent<PaddleController>();

        paddle.RegisterPaddleActivated(paddleActivated);
        outOfBounds_GO.GetComponent<OutOfBounds>().RegisterOutOfBounds(outOfBoundsTrigger);
    }

    // Update is called once per frame
    void Update() {
        foreach (LightSensor lightSensor in sensors) {
            lightSensor.update();
        }

        if (travelTimer) {
            travelTime += Time.deltaTime;
        }
    }

    // Gets called whenever ANY Sensor changes
    void OnSensorChanged(LightSensor lightSensor) {
        if (hitCalculated) {
            return;
        }

        if (lightSensor1 == null) {
            lightSensor1 = lightSensor;
            travelTimer = true;
            return;
        }

        // If we read the same sensor twice return.
        if (lightSensor1 == lightSensor) {
            return;
        }

        // If the ball is moving upwards, we discard the results.
        if (lightSensor1.sensor.transform.position.z < lightSensor.sensor.transform.position.z) {
            lightSensor1 = null;
            return;
        }

        // The ball have moved horisontally in the matrix. Meaning that two sensors with different x cordinates have triggered.
        // From this we can calculate the angle the ball is traveling at... ish.
        if (lightSensor1.sensor.transform.position.x != lightSensor.sensor.transform.position.x) {
            travelTimer = false;

            calculateHit(lightSensor1, lightSensor);
            return;
        }

        // The ball have only moved verticly in the matrix. Meening that two sensors with the same x cordinate have triggered.
        // From this we can assume that the angle the ball is traveling at is 0... ish.
        else if (lightSensor1.sensor.transform.position.z > lightSensor.sensor.transform.position.z) {
            travelTimer = false;

            calculateHit(lightSensor1, lightSensor, true);
            return;
        }
    }

    void calculateHit(LightSensor sensor1, LightSensor sensor2, bool vertical = false) {

        if (hitCalculated) {
            return;
        }

        Debug.Log("hitCalced");

        hitCalculated = true;

        Vector3 sensor1Pos = sensor1.sensor.transform.position;
        Vector3 sensor2Pos = sensor2.sensor.transform.position;
        
        lightSensor1 = null;

        // Z is always positive. As a negative z means the ball is moving upwards wich we discard
        float distanceTravveledInZ = sensor1Pos.z - sensor2Pos.z;
        float zPerSecond = distanceTravveledInZ / travelTime;

        if (vertical) {

            // The ball is now at sensor 2, ich.

            float distToPaddle_z = Mathf.Abs(sensor2Pos.z + transform.position.z - paddle_GO.transform.position.z);

            paddle.activate(distToPaddle_z / zPerSecond);

            travelTime = 0f;
        }

        else {

            // Z is always positive, X migth be negative

            float distanceTravveledInX = Mathf.Abs(sensor1Pos.x - sensor2Pos.x);

            float xPerSecond = distanceTravveledInX / travelTime;

            // The ball is now at sensor 2, ich.

            float distToPaddle_x = Mathf.Abs(sensor2Pos.x + transform.position.x - paddle_GO.transform.position.x);
            float distToPaddle_z = Mathf.Abs(sensor2Pos.z + transform.position.z - paddle_GO.transform.position.z);

            paddle.activate(distToPaddle_z / zPerSecond);

            travelTime = 0f;

        }
    }

    void outOfBoundsTrigger(OutOfBounds outOfBounds) {
        lightSensor1 = null;
        hitCalculated = false;
    }

    void paddleActivated(PaddleController paddleController) {
        hitCalculated = false;
    }
}


// Optimizations*
// If the flipper has been activated, stop updating the sensors. Will cause fps spikes though...

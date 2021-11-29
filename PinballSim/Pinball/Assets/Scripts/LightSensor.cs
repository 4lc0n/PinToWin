/*
 * This script controlls a ligthsensor.
 */

using System;
using UnityEngine;

public class LightSensor
{

    // Raycast to check for the ball.
    RaycastHit raycast;

    // the gameobject that this sensor is attached to.
    public GameObject sensor { get; protected set; }
    
    // Timer used to change the color of the sensor.
    float colorTime = 0f;
    bool timer = false;

    // Constructor.
    public LightSensor(GameObject sensor) {
        this.sensor = sensor;
    }

    public void update() {
        // Bit shift the index of the layer (8) to get a bit mask
        int layerMask = 1 << 8;

        // This would cast rays only against colliders in layer 8.
        // But instead we want to collide against everything except layer 8. The ~ operator does this, it inverts a bitmask.
        layerMask = ~layerMask;

        // Does the ray intersect any objects excluding the player layer.
        if (Physics.Raycast(sensor.transform.position, sensor.transform.TransformDirection(new Vector3(0, 0, -1)), out raycast, 10, layerMask)) {

            //If so is it the ball.
            if (raycast.collider.tag == "Ball") {
                // If so. Change the color of the sensor.
                sensor.GetComponent<SpriteRenderer>().color = Color.red;

                // Call the callback.
                cbSensorChanged(this);

                timer = true;
            }
        }

        if (colorTime > 2) {
            sensor.GetComponent<SpriteRenderer>().color = Color.green;
            colorTime = 0;
        }

        if (timer) {
            colorTime += Time.deltaTime;
        }
    }

    Action<LightSensor> cbSensorChanged;

    public void RegisterSensorChanged(Action<LightSensor> callbackfunc) {
        cbSensorChanged += callbackfunc;
    }
}

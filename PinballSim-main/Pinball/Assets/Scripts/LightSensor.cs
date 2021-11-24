using System.Collections;
using System.Collections.Generic;
using System;
using UnityEngine;

public class LightSensor
{

    RaycastHit raycast;

    public GameObject sensor { get; protected set; }
    
    float colorTime = 0f;
    bool timer = false;

    public LightSensor(GameObject sensor) {
        this.sensor = sensor;
    }

    public void update() {
        // Bit shift the index of the layer (8) to get a bit mask
        int layerMask = 1 << 8;

        // This would cast rays only against colliders in layer 8.
        // But instead we want to collide against everything except layer 8. The ~ operator does this, it inverts a bitmask.
        layerMask = ~layerMask;

        // Does the ray intersect any objects excluding the player layer
        if (Physics.Raycast(sensor.transform.position, sensor.transform.TransformDirection(new Vector3(0, 0, -1)), out raycast, 10, layerMask)) {

            if (raycast.collider.tag == "Ball") {
                sensor.GetComponent<SpriteRenderer>().color = Color.red;

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

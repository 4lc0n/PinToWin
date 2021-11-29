/*
 * This script is used to controll a LightSensor matrix.
 */

using UnityEngine;

public class Matrix : MonoBehaviour
{
    // The flipper above this matrix.
    public GameObject paddle_GO;
    PaddleController paddle;

    // 2-dimetional array of the lightsensors in this matrix
    LightSensor[,] sensors;

    // Sprite attached to the sensors. Just for visuals.
    // Note: the sensors occupy just a single point. the provided sprite is 100 times larger than the sensors coverage.
    public Sprite sprite;

    LightSensor lightSensor1;

    // Store the balls traveltime between 2 points.
    float travelTime;

    bool travelTimer;

    // hitCalculated will be set to true when a hit is calculated, and false when the flipper activates. This is for optimazation
    bool hitCalculated;

    // Start is called before the first frame update
    void Start() {
        int size = 10;
        float t = size / 5;
        sensors = new LightSensor[size, size];

        // Create all the LightSensors and give them a position.
        /// FIXME: Probably broken by the boards dimentions change.

        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {

                // Create a new GameObject.
                GameObject gameObject = new GameObject("Sensor_" + x + "_" + y);
                gameObject.transform.SetParent(transform);
                gameObject.transform.position = new Vector3(transform.position.x + (x / t), 0.1f, transform.position.z + (y / t));
                gameObject.transform.rotation = Quaternion.AngleAxis(90, new Vector3(1, 0, 0));

                // Attahc a SpriteRenderer to the GameObject.
                SpriteRenderer renderer = gameObject.AddComponent<SpriteRenderer>();
                renderer.sprite = sprite;
                renderer.color = Color.green;

                // Construct a nex LightSensor;
                sensors[x, y] = new LightSensor(gameObject);

                // Add the LightSensor to the LightSensor array.
                sensors[x, y].RegisterSensorChanged(OnSensorChanged);
            }
        }

        // Fetch the flipper controller script of the flipper above this matrix.
        paddle = paddle_GO.GetComponent<PaddleController>();

        // Subscribe to some callbacks.
        paddle.RegisterPaddleActivated(paddleActivated);
        OutOfBounds.outOfBounds.RegisterOutOfBounds(outOfBoundsTrigger);
    }

    // Update is called once per frame
    void Update() {

        // Update each sensor each frame.
        foreach (LightSensor lightSensor in sensors) {
            lightSensor.update();
        }

        // Increment travelTime by Time.deltaTime if we are trying to calcuale the balls speed.
        if (travelTimer) {
            travelTime += Time.deltaTime;
        }
    }

    // Gets called whenever ANY Sensor changes
    void OnSensorChanged(LightSensor lightSensor) {

        // If a hit has allready been calculated just return. For optimazation.
        if (hitCalculated) {
            return;
        }

        // If this is the first sensor the ball hits, lightSensor1 will benull.
        if (lightSensor1 == null) {

            // Set lightSensor1 to this lightSensor.
            lightSensor1 = lightSensor;

            // Start the timer to time the traveltime and return.
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

            // Reset lightSensor1.
            lightSensor1 = null;
            return;
        }

        // The ball have only moved verticly in the matrix. Meening that two sensors with the same x cordinate have triggered.
        // From this we can assume that the angle the ball is traveling at is 0... ish.
        else if (lightSensor1.sensor.transform.position.z > lightSensor.sensor.transform.position.z) {
            travelTimer = false;

            // Call calculateHit to actually do the calculations.
            calculateHit(lightSensor1, lightSensor, true);
            return;
        }
    }

    /// <summary>
    /// Function to calculate different ball parameters based on 2 sensor readings.
    /// </summary>
    /// <param name="sensor1">First sensor</param>
    /// <param name="sensor2">Second sensor</param>
    /// <param name="vertical">Set this to true if the ball is moving vertically. For optimazation.</param>
    void calculateHit(LightSensor sensor1, LightSensor sensor2, bool vertical = false) {

        // If a hit has allready been calculated just return. For optimazation.
        if (hitCalculated) {
            return;
        }

        // The ball is now at sensor 2, ich.

        // Set hitCalculated to true for optimazation.
        hitCalculated = true;

        // Fetch the true positions of the 2 sensors.
        Vector3 sensor1Pos = sensor1.sensor.transform.position;
        Vector3 sensor2Pos = sensor2.sensor.transform.position;

        // Z is always positive. As a negative z means the ball is moving upwards wich we discard.
        // Find the distance the ball has travveled in the Z axis.
        float distanceTravveledInZ = sensor1Pos.z - sensor2Pos.z;

        // Find the speed the ball has in the Z axis.
        float zPerSecond = distanceTravveledInZ / travelTime;

        // If the ball is moving just-ich vertically. We don't have to worry about the horizontal direction.
        if (vertical) {

            // The ball is now at sensor 2, ich.

            // Crudely calculate the distance to the flipper.
            float distToPaddle_z = Mathf.Abs(sensor2Pos.z + transform.position.z - paddle_GO.transform.position.z);

            // Activate the paddle adn send with a delay of distToPaddle_z / zPerSecond.
            paddle.activate(distToPaddle_z / zPerSecond);

            // Reset the traveltime.
            travelTime = 0f;
        }

        else {
            // The ball is not moving just vertically.

            // Find the distance the ball has travveled in the X axis. X migth be negative so we use the aboslute value.
            float distanceTravveledInX = Mathf.Abs(sensor1Pos.x - sensor2Pos.x);

            // Find the speed the ball has in the X axis.
            float xPerSecond = distanceTravveledInX / travelTime;

            // Crudely calculate the distance to the flipper.
            float distToPaddle_x = Mathf.Abs(sensor2Pos.x + transform.position.x - paddle_GO.transform.position.x);
            float distToPaddle_z = Mathf.Abs(sensor2Pos.z + transform.position.z - paddle_GO.transform.position.z);

            // Activate the paddle adn send with a delay of distToPaddle_z / zPerSecond.
            paddle.activate(distToPaddle_z / zPerSecond);

            // Reset the traveltime.
            travelTime = 0f;

            /// TODO: Actually use the horizontal distance for something.
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

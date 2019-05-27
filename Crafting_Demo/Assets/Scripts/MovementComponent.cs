using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MovementComponent : MonoBehaviour
{
    float tMoveRight = 0.0f;
    float tMoveForward = 0.0f;

    float tYaw = 0.0f;
    float tPitch = 0.0f;

    private void Start()
    {
        //Cursor.lockState = CursorLockMode.Locked;
    }

    void Update()
    {
        float tMoveSpeed = 100000.0f * Time.deltaTime;
        float tRotSpeed = 2.0f;

        tMoveRight = 0.0f;
        tMoveForward = 0.0f;


        tYaw += tRotSpeed * Input.GetAxis("Mouse X");
        tPitch -= tRotSpeed * Input.GetAxis("Mouse Y");
        Vector3 tPos = new Vector3(tMoveRight, 0, tMoveForward);
        tPos.Scale(transform.forward);
        transform.eulerAngles = new Vector3(tPitch, tYaw, 0);
        
        var tCam = GetComponentInChildren<Camera>();
        var tCamForward = tCam.transform.forward;
        tCamForward.y = 0;
        var tCamRight = tCam.transform.right;

        Rigidbody tRigidBody = GetComponent<Rigidbody>();

        if (Input.GetKey("a"))
        {
            tRigidBody.AddForce(tCamRight * -tMoveSpeed);
            //transform.position -= tCamRight * tMoveSpeed;
        }
        if (Input.GetKey("d"))
        {
            tRigidBody.AddForce(tCamRight * tMoveSpeed);
            //transform.position += tCamRight * tMoveSpeed;
        }
        if (Input.GetKey("w"))
        {
            tRigidBody.AddForce(tCamForward * tMoveSpeed);
            //transform.position += tCamForward * tMoveSpeed;
        }
        if (Input.GetKey("s"))
        {
            tRigidBody.AddForce(tCamForward * -tMoveSpeed);
            //transform.position -= tCamForward * tMoveSpeed;
        }
        


    }
}

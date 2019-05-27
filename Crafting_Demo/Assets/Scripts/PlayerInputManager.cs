using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityStandardAssets.Characters.FirstPerson;

public class PlayerInputManager : MonoBehaviour
{
    // Update is called once per frame
    void Update()
    {
        if (Input.GetButtonDown("Inventory"))
        {
            ToggleInventory();
        }
        else if (Input.GetButtonDown("Interact"))
        {
            CheckRaycast();
        }
    }

    private void ToggleInventory()
    {
        RigidbodyFirstPersonController playerController = GameObject.Find("PlayerPawn").GetComponent<RigidbodyFirstPersonController>();

        if (UIManager.Get().GetCurrentScreen().name == "HUD")
        {
            UIManager.Get().ShowScreen("Inventory");
            Cursor.visible = true;
            Cursor.lockState = CursorLockMode.None;
            playerController.enabled = false;
            Time.timeScale = 0f;

            InventoryScreen inventoryScreen = UIManager.Get().GetCurrentScreen().GetComponent<InventoryScreen>();

            inventoryScreen.Initialise();
        }
        else
        {
            UIManager.Get().ShowScreen("HUD");
            Cursor.visible = false;
            Cursor.lockState = CursorLockMode.Locked;
            playerController.enabled = true;
            Time.timeScale = 1f;
        }
    }

    private void CheckRaycast()
    {
        Camera camera = GetComponentInChildren<Camera>();
        Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
        RaycastHit hit;
        if (Physics.Raycast(ray, out hit))
        {
            ItemActor itemActor = hit.collider.GetComponent<ItemActor>();
            if (itemActor)
            {
                Inventory inventory = GetComponent<Inventory>();
                inventory.AddItemToInventory(itemActor.GetItemData());

                Destroy(hit.collider.gameObject);
            }
        }
    }
}
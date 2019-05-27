using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityStandardAssets.Characters.FirstPerson;


public class PlayerController : MonoBehaviour
{
    PlayerInputManager m_InputManager;
    RigidbodyFirstPersonController m_MovementController;
    Inventory m_Inventory;

    private GameObject m_PlayerPawn;

    // Start is called before the first frame update
    void Start()
    {
        m_InputManager = GetComponent<PlayerInputManager>();
        m_MovementController = GetComponent<RigidbodyFirstPersonController>();
        m_Inventory = GetComponent<Inventory>();
        UIManager.Get().ShowScreen("HUD");
    }

    public GameObject GetPawn()
    {
        return m_PlayerPawn == null ? (m_PlayerPawn = gameObject) : m_PlayerPawn;
    }
}

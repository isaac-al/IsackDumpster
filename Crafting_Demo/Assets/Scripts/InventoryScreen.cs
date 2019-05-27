using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class InventoryScreen : MonoBehaviour
{
    public GameObject ButtonPrefab;

    private GameObject m_ItemWrapper;
    private Inventory m_Inventory;

    public void Initialise()
    {
        m_ItemWrapper = GameObject.Find("ItemWrapper");

        m_ItemWrapper.transform.Clear();

        m_Inventory = GameStatics.GetPawn().GetComponent<Inventory>();

        foreach (var item in m_Inventory.GetInventoryItems())
        {
            GameObject button = Instantiate(ButtonPrefab);
            button.transform.SetParent(m_ItemWrapper.transform);
            Text buttonText = button.GetComponentInChildren<Text>();
            buttonText.text = item.Name;
        }
    }
}
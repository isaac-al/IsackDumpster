using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ItemActor : MonoBehaviour
{
    public string ItemName;

    private Item m_Item;

    // Start is called before the first frame update
    void Start()
    {
        m_Item = ItemManager.GetListItemByName(ItemName);
    }

    public Item GetItemData()
    {
        return m_Item;
    }
}
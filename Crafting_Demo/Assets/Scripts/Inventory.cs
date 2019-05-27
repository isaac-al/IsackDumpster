using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Inventory : MonoBehaviour
{
    protected List<Item> m_ItemList;

    public List<string> GetItemsAsKeyList()
    {
        List<string> tKeyList = new List<string>();

        foreach (var tItem in m_ItemList)
        {
            tKeyList.Add(tItem.Key);
        }

        return tKeyList;
    }

    public void AddItemToInventory(Item pItem)
    {
        if (m_ItemList == null) m_ItemList = new List<Item>();

        m_ItemList.Add(pItem);
    }

    public List<Item> GetInventoryItems()
    {
        if (m_ItemList == null) m_ItemList = new List<Item>();

        return m_ItemList;
    }

    // TODO : Make inventory GUI - grid list of items (use text as image placeholder)
}

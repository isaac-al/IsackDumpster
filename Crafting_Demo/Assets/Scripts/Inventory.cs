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

    public void AddItemToInventory(Item _item)
    {
        if (m_ItemList == null) m_ItemList = new List<Item>();

        m_ItemList.Add(_item);
    }

    public List<Item> GetInventoryItems()
    {
        if (m_ItemList == null) m_ItemList = new List<Item>();

        return m_ItemList;
    }

    public void RemoveItem(Item _item)
    {
        m_ItemList.Remove(_item);
    }

    public void RemoveItem(string _name)
    {
        m_ItemList.Remove(m_ItemList.Find(x => x.Name.Equals(_name)));
    }
}

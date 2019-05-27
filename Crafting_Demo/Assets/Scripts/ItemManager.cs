using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class ItemManager
{
    /// <summary>
    /// SINGLETON IMPLEMENTATION
    /// </summary>
    
    private static ItemManager m_Instance = null;
    public List<Item> m_ItemList;
            
    public static ItemManager Get()
    {
        if (m_Instance == null)
        {
            m_Instance = new ItemManager();
            m_Instance.Initialise();
        }

        return m_Instance;
    }

    internal void Initialise()
    {
        m_ItemList = new List<Item>();
    }

    public static List<Item> GetItemList()
    {
        return Get().m_ItemList;
    }

    public static void SetItemList(List<Item> pItemList)
    {
        Get().m_ItemList = pItemList;
    }

    public static void AddListItem(Item pInItem)
    {
        Get().m_ItemList.Add(pInItem);
    }

    public static Item GetListItem(Item pItem)
    {
        return Get().m_ItemList.Find(x => x == pItem);
    }

    public static Item GetListItemByKey(string pKey)
    {
        return Get().m_ItemList.Find(x => x.Key.Equals(pKey));
    }

    public static Item GetListItemByName(string pName)
    {
        return Get().m_ItemList.Find(x => x.Name.Equals(pName));
    }
}


using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CraftingManager : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        //Item tWood = ItemManager.GetListItemByName("Wood");
        //Item tStone = ItemManager.GetListItemByName("Stone");

        //Item tCraftedItem = ItemManager.GetListItemByKey(tWood.Key + tWood.Key + tStone.Key);

        //CraftItem(tCraftedItem);

        //Debug.Log(tCraftedItem.Name);
    }

    // Splits key and checks items are in inventory, if so adds to inventory
    bool CraftItem(Item pItem)
    {
        Inventory tInventory = GetComponent<Inventory>();
        var tKeyArr = StringHelper.SplitCamelCase(pItem.Key);

        // TODO: Get all item keys in inventory
        string[] tInventoryArr = { "Aa", "Z", "B", "Aa", "Ba", "Yoi" };
        List<string> tInventoryList = new List<string>(tInventoryArr);
        
        for (int i = 0; i < tKeyArr.Count; ++i)
        {
            for (int j = 0; j < tInventoryList.Count; ++j)
            {
                if (tKeyArr[i] == tInventoryList[j])
                {
                    tKeyArr[i] = " ";
                    tInventoryList[j] = " ";
                }

                tInventoryList.RemoveAll(x => x == " ");
            }
        }

        tKeyArr.RemoveAll(x => x == " ");

        for (int i = 0; i < tKeyArr.Count; ++i)
        {
            Debug.Log(tKeyArr[i]);
        }

        Debug.Log(tKeyArr.ToString());

        if (tKeyArr.Count <= 0)
        {
            // Add pItem to inventory
            Debug.Log("SUCCESSFULLY CRAFTED");
            return true;
        }

        Debug.Log("CRAFTING FAILED");

        return false;
    }
}



using System.Collections;
using System.Collections.Generic;
using UnityEngine;


[System.Serializable]
public class Item
{
    public string Name;
    public string Key;

    public Item(string pKey, string pName)
    {
        Key = pKey;
        Name = pName;
    }
}

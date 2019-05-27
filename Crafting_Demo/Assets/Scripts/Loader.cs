using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;


public class Loader : MonoBehaviour
{
    // Start is called before the first frame update
    void Awake()
    {
        ItemManager.SetItemList(LoadItemListFromFile());

        //foreach (var tItem in ItemManager.GetItemList())
        //{
        //    Debug.Log(tItem.Name);
        //}
    }

    private List<Item> LoadItemListFromFile()
    {
        string tRawData = LoadDataFromFile("/Items.json");
        Item[] tItemList = JsonHelper.FromJson<Item>(tRawData); 
        return new List<Item>(tItemList);
    }

    public static string LoadDataFromFile(string pPath)
    {
        string tFileData = " ";
        string tPath = Application.streamingAssetsPath + pPath;
        if (Application.platform == RuntimePlatform.Android)
        {
            WWW tScanner = new WWW(tPath);
            while (!tScanner.isDone) { }

            tFileData = tScanner.text;
        }
        else
        {
            tFileData = File.ReadAllText(tPath);
        }

        return tFileData;
    }

    // Update is called once per frame
    void Update()
    {

    }
}

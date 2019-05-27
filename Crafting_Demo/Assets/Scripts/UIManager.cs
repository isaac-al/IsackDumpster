using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UIManager
{
    private static UIManager m_UIManager = null;

    public List<GameObject> m_PanelList;

    public GameObject CurrentScreen;
    public GameObject CanvasMain;
 
    public static UIManager Get()
    {
        if (m_UIManager == null)
        {
            m_UIManager = new UIManager();
            m_UIManager.Initialise();
        }

        return m_UIManager;
    }

    internal void Initialise()
    {
        CanvasMain = GameObject.Find("CanvasMain");

        m_PanelList = new List<GameObject>();

        for (int i = 0; i < CanvasMain.transform.childCount; ++i)
        {
            GameObject panel = CanvasMain.transform.GetChild(i).gameObject;

            if (panel.GetComponent<CanvasGroup>())
            {
                panel.GetComponent<CanvasGroup>().alpha = 0;
            }

            m_PanelList.Add(panel);
        }
    }

    public void TogglePause()
    {

    }

    public void ShowScreen(string _name)
    {
        GameObject screenObj = FindScreenByName(_name);

        ScreenBehaviour screen = screenObj.GetComponent<ScreenBehaviour>();

        screen.SetVisibility(true);

        if (CurrentScreen)
        {
            ScreenBehaviour prevBehaviour = CurrentScreen.GetComponent<ScreenBehaviour>();

            prevBehaviour.SetVisibility(false);
        }

        CurrentScreen = screenObj;
    }

    public GameObject FindScreenByName(string _name)
    {
        return m_PanelList.Find(x => x.name.Equals(_name));
    }

    public GameObject GetCurrentScreen()
    {
        return CurrentScreen;
    }
}

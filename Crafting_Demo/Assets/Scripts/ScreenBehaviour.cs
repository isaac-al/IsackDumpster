using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ScreenBehaviour : MonoBehaviour
{
    public void SetVisibility(bool _visibility)
    {
        if (GetCanvasGroup())
        {
            GetCanvasGroup().alpha = _visibility ? 1 : 0;
        }
    }

    public CanvasGroup GetCanvasGroup()
    {
        return GetComponent<CanvasGroup>();
    }

    public bool IsVisible()
    {
        if (GetCanvasGroup())
        {
            return GetCanvasGroup().alpha == 1 ? true : false;
        }

        return false;
    }
}

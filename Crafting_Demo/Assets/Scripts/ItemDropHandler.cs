using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class ItemDropHandler : MonoBehaviour, IDropHandler
{
    public void OnDrop(PointerEventData eventData)
    {
        RectTransform invRect = transform.parent.transform as RectTransform;

        if (!RectTransformUtility.RectangleContainsScreenPoint(invRect,
            Input.mousePosition, null))
        {
            Debug.Log("Item dropped");

            Destroy(transform.gameObject);

            Inventory inventory = GameStatics.GetPawn().GetComponent<Inventory>();

            inventory.RemoveItem(name);
        }
    }
}

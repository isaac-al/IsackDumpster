using System.Collections;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using UnityEngine;

public class StringHelper
{
    public static List<string> SplitCamelCase(string source)
    {
        return new List<string>(Regex.Split(source, @"(?<!^)(?=[A-Z])"));
    }
}

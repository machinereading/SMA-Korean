# SMA for Korean

## Description

This software is a morphological analyzer for Korean text

## How to use

Input (input.txt) is raw text

```
./sma_run.sh input.txt output.txt
```

For example, input.txt is:

```
반기문은 1944년 6월 13일에 태어났다.
```

output.txt is:

```
반기문은 1 반기문/NNP+은/JX 57.21269 반기/NNG+문/NNG+은/JX 66.11838 반기/VV+문/EC+은/JX 68.44003 반기문/NNP+은/NNG 68.69556 반기문/NNP+은/JKS 69.64169 반기문/NNP+은/NNP 71.21458 반기/NNG+문/NNB+은/JX 72.98160 반기/NNG+문/NNP+은/JX 73.19745 반/XPN+기/NNG+문/NNG+은/JX 75.95337 반기/VV+문/EC+은/ETM 76.77135

1944년 0 1944/SN+년/NNB 1.00000

6월 0 6/SN+월/NNB 1.00000

13일에 0 13/SN+일/NNB+에/JKB 1.00000

태어났다. 1 태어났다./NNG 51.40769
```


## Licenses

* `CC BY-NC-SA` [Attribution-NonCommercial-ShareAlike](https://creativecommons.org/licenses/by-nc-sa/2.0/)
* If you want to commercialize this resource, [please contact to us](http://mrlab.kaist.ac.kr/contact)


To apply the patches, from the root of this project:

```
cd components/btstack
git apply ../patches/*.patch
```

And after that, you have to install btstack:

```
cd port/esp32
./integrate_btstack.py
```
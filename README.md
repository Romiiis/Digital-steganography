# <p align=center> Digital steganography</p>
<p align="center">:computer: Hide and extract secrets into/from images!</p>
</br>

 ## :smiling_imp: What does this application do ? 
 This CLI application can hide a payload in a bmp/png image and then extract it from it. </br></br>

## :minidisc: Dependencies:
This application requires libpng (http://www.libpng.org/pub/png/libpng.html) in system PATH
If you work on windows install make for windows (https://gnuwin32.sourceforge.net/packages/make.htm)

## :eyes: How to install ? 
 ### Just type this in console:
```
make
```

## :herb: Usage
 ### Structure of command is:
```
stegim.exe <image[.png|.bmp]> <-switch> <payload>
```
Where
<ul style="list-style-type: square;">
  <li> -switch
  <ul style="list-style-type: square;">
    <li>-x (extract)</li>
    <li>-h (hide)</li>
  </ul>
</li>
  <li>image[.png|.bmp]
    <ul style="list-style-type: square;">
      <li>Image where to hide payload (-h)</li>
      <li>Image where is payload already hidden (-x)</li>
    </ul>
</li>
<li> payload
  <ul style="list-style-type: square;">
    <li>Secret that you want to hide (-h)</li>
    <li>Where you want to save payload from picture (-x)</li>
  </ul>
</li>

</br>

## :clipboard: Examples:
  ### Hide payload:
  ```
  stegim.exe img.bmp -h secret.txt
  ```
  ### Extract it:
  ```
  stegim.exe img.bmp -x whatIsInImg.txt
  ```
## :scissors: Error codes
<table align="center">
  <tr>
    <th>Return code</th>
    <th>Error description</th>
  </tr>
  <tr>
    <td>0</td>
    <td>No error</td>
  </tr>
  <tr>
    <td>1</td>
    <td>incorrectly entered or missing command line parameters (also non-existent input/output files)</td>
  </tr>
  <tr>
    <td>2</td>
    <td>inappropriate image format (i.e. not PNG or BMP, 24-bit/pixel RGB)</td>
  </tr>
  <tr>
    <td>3</td>
    <td>the image is not large enough to hide the specified payload and the necessary accompanying information</td>
  </tr>
  <tr>
    <td>4</td>
    <td>the payload extraction image does not contain any hidden content</td>
  </tr>
  <tr>
    <td>5</td>
    <td>the hidden content has been corrupted, the content checksum does not match, or the content cannot be decompressed correctly</td>
  </tr>
  <tr>
    <td>6</td>
    <td>Any other error</td>
  </tr>
</table>


</br>

___
<p align="center">This project was written within the KIV/PC course at the Faculty of Applied Sciences at the University of West Bohemia in Pilsen </p>

 


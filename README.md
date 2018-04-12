TSK-js
=======================
A module that uses The Sleuth Kit to make some disk analysis on Javascript

## Installation

For a complete guide on the installation process go to
[Installation](./static/installation.md) chapter

### Requirements

[The Sleuth Kit](https://www.sleuthkit.org/sleuthkit/download.php) must be installed before downloading this package.

### Module

You can install it just using the command:

```bash
$ npm install tsk-js --save
```

## Usage example

This is an example of a script that performs a brief analysis. To learn how to 
use it in more detail go to [User guide](./static/guide.md) section.

[//]: # (TODO: Provide the image to execute this example)

```javascript
const { TSK } = require("tsk-js");
analyzeImage("hdd-001.dd")

////

function searchRecursive(needle, img, imgaddr, inode, cb) {
    // Retrieve files in current folder
    const files = img.list({ imgaddr, inode });

    // Process 
    files
        .filter((f) => f.name === needle)
        .forEach((f) => cb(f));
 
    files
        .filter((f) => f.type === "directory")
        .forEach((f) => searchRecursive(needle, img, imgaddr, f.inode, cb));
}

function analyzePartition(img, imgaddr) {
    // Search file
    searchRecursive("carta.txt", img, imgaddr, undefined, (file) => {
        const { inode } = file;
	    const buff = img.get({ imgaddr, inode });

        console.log("File found!");
	    console.log("Print it's content:");
        console.log("---------------------------");
        console.log(buff.toString());
        console.log("---------------------------");
    });

    // Generate timeline
    const timeline = img.timeline(() => {}, { imgaddr });
    console.log(timeline.length);
}

function analyzeDisk(img, res) {
    res.partitions
        .filter((p) => p.hasFs)
	    .forEach((p) => analyzePartition(img, p.start));
}

function analyzeImage(imgfile) {
    const img = new TSK("hdd-001.dd");
    const res = img.analyze();
    if (res.type === "disk") {
        analyzeDisk(img, res);
    } else {
        analyzePartition(img, 0);
    }
}
```
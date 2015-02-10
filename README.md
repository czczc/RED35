# ROOT-based Event Display for LBNE-35t

## Instructions

### Dowload the Event Viewer 

    git clone https://github.com/czczc/RED35.git
    cd RED35

### Prepare Data 

(on the cluster where you do the simulation and reconstruction)

* git flow feature pull origin chaoz_ctree
* re-compile lbnecode
* produce a MC/reco root file
* lar -c ctree35t.fcl [MC/reco]


This will generate a "sample.root" file for the event display. You can then scp this file to your local [data/](https://github.com/czczc/RED35/tree/master/data) directory.

### Build the Application
    cd scripts
    root -b -q -l loadClasses.C

### 2D Event Display
    (under the scripts/ directory)
    root -l run2d.C

### 3D Event Display
    (under the scripts/ directory)
    root -l run3d.C

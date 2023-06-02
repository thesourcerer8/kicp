# The Knight Industries Co-Process

![Knight Industries Co-Process](knight.png)

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0) [![UPRJ_CI](https://github.com/efabless/caravel_project_example/actions/workflows/user_project_ci.yml/badge.svg)](https://github.com/efabless/caravel_project_example/actions/workflows/user_project_ci.yml) [![Caravel Build](https://github.com/efabless/caravel_project_example/actions/workflows/caravel_build.yml/badge.svg)](https://github.com/efabless/caravel_project_example/actions/workflows/caravel_build.yml)

This is meant to be a simple prototype and proof of concept of an AI accelerator.

Due to the limited space, I can't have endless memory on there, so I'll at best be able to run some NanoGPT for now.

Long term, I wanna hook up a DRAM controller to that block and run a full Vicuna or GPT2.

The design is based on the 24 hour output from BabyAGI as well as a lot of very lovely help from ChatGPT

    wget https://pdk.libresilicon.com/~leviathan/BabyAGI_silicon.log

I first have to cleanup the chatlogs with ChatGPT before I can upload those too, our discussions sometimes
become really deep and meta.

Planned are several features like matrix multiplication, matrix convolution and so on... so far I've just got multiplication,
because the AI generated chip challenge only gave me a week.

Features...

## Matrix Multiplication

It takes in two matrices A and B, multiplies them and spits out the matrix C

## Matrix convolution

WIP... maybe I still manage, but people need sleep...


Refer to [README](docs/source/index.rst#section-quickstart) for a quickstart of how to use caravel_user_project

Refer to [README](docs/source/index.rst) for this sample project documentation. 

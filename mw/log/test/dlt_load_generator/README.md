<!--- 
*******************************************************************************>
Copyright (c) 2024 Contributors to the Eclipse Foundation
See the NOTICE file(s) distributed with this work for additional
information regarding copyright ownership.
This program and the accompanying materials are made available under the
terms of the Apache License Version 2.0 which is available at
https://www.apache.org/licenses/LICENSE-2.0
SPDX-License-Identifier: Apache-2.0 #
*******************************************************************************
 ---> 



# DLT load generator program

Simple program to stimulate datarouter with a target bandwidth in case of verbose or non-verbose logging.

## Usage

1. Choose verbose logging by setting flag VERBOSE_LOGGING=1 in dlt_load_generator binary bazel rule and by default it's non verbose.

2. Build the package and upload it to the running target in case of verbose logging.

```bash
bazel build --config=ipnext_arm64_qnx //platform/aas/mw/log/test/dlt_load_generator:dlt_load_generator-pkg
scp  bazel-bin/platform/aas/mw/log/test/dlt_load_generator/dlt_load_generator-release-pkg.tar target:/persistent
```

3. *In case of non verbose*
- Choose no_verbose logging by setting flag VERBOSE_LOGGING=0 in dlt_load_generator binary bazel rule.

- Put non_verbose_golden_app_ids.json under ecu/xpad/xpad-shared/config/common/pas/logging 
- Add the following in ddad/ecu/xpad/xpad-shared/config/common/pas/logging/BUILD

```
filegroup(
    name = "non_verbose_app_ids",
    srcs = ["non_verbose_golden_app_ids.json"],
    visibility = ["//visibility:public"],
)
```
- Adjust common_fixed_ids in ddad/ecu/xpad/xpad-shared/config/common/pas/logging/BUILD to have non_verbose_app_ids.json

- Under ecu/xpad/xpad-shared/packaging/swe/swfl_platform/BUILD edit platform_apps_common to have 
"//platform/aas/mw/log/test/non_verbose_dlt_load_app:pkg",

- Disable running of EM from startup scripts ecu/xpad/xpad-shared/config/os/fs/init/usr/sbin/bmw_startup.sh


- Build IPNext image 

```bash
bazel build --config=ipnext_arm64_qnx --config=ad_gen_25 //ecu/xpad/xpad-shared/packaging/image:IPNext_HLOS
```

4. Restart datarouter to ensure other processes are not interfering with the test in case of verbose logging and for non verbose logging just run the data router since EM is disabled to have free cross interference environment.

```bash
ssh target
slay datarouter
cd /bmw/platform/opt/datarouter && on -u 1038:1036  -A nonroot,allow,pathspace ./bin/datarouter -T datarouter_t --no_adaptive_runtime &
```

5. In a separate terminal start the DLT load program to stimulate the required bandwidth.

```bash
ssh target
cd /persistent
tar -xvf dlt_load_generator-release-pkg.tar
cd opt/dlt_load_generator
./bin/dlt_load_generator 10 # Stimulate with 10 MB/s DLT payload
```

6. Measure memory and cpu usages 

```bash
showmem -p proc_id 
top
```

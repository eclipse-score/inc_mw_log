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



# FileOutputBackend

`LogFlushBackend` implements `mw::log::Backend`. It is responsible for providing
buffers by means of `ReserveSlot()` call and writing data using provided
callable object when the buffer is returned by the user by means of calling
`FlushSlot()`. It assumes non-blocking calls and implements measures to store
and manage buffers till all the data is pushed out in subsequent calls. Logs
are stored in a circular buffer in the case when the current `LogRecord` is not
completely flushed due to provided sink method not consuming all the data. The
rest of the data is to be pushed in subsequent calls to `FlushSlot` looping over
a circular buffer to reduce the queue of unprocessed logs in the order provided
by the caller. Whenever each `LogRecord` is flushed completely, the buffer is
released to the allocator by Release Slot.
In case of depletion of a fixed number of buffers, none is returned to the
caller rather than overwriting old ones.
The zero-copy approach was used inside the backend design. Circular buffers
store objects of `SlotHandle` type which is just a handle to buffers storing
data. It is part of the job of the Recorder to format and copy content into the
buffers.
The zero-copy principle does not hold for the data provided at a level of and
above Recorder.
Configuration of the backend is done through resources passed down in
constructor arguments consisting of slot allocator and circular buffer.

Design decision has been made to make backend independent of loosely coupled
to details of data it processes which is presented by `LogRecord` type. To do
so object providing `IMessageBuilder` interface is passed down by Recorder to
allow access data in polymorphic way. This interface makes it possible to access
set of sequential data buffers in order they should be serialized into a file
or onto the console.
`MessageBuilder` is responsible for serving data in correct order of flushing
into file or console and stores some of common parts of the header.

![Static Design](/swh/ddad_platform/aas/mw/log/design/mw_log_file_backend.uxf?ref=a0f7d7e092a6d561d0c889a2faf752acc969f474)

`SlotDrainer` is responsible for storing and disposal of already serialized
data. First data gets inserted into circular buffer. After that step program
flow enters a loop that iterates over available slots in ring buffer and then
by means of `NonBlockingWriter` iterates over all spans of each message.

![Sequence Design](/swh/ddad_platform/aas/mw/log/design/slot_drainer_sequence_design.uxf?ref=be7b21852077fc3db76686d352d9907ec8495a08)

Flush procedure exits whenever all available data is written to the file or
writing procedure would block i.e. write operation reports that number of bytes
written is less then requested.

![Action Diagram](/swh/ddad_platform/aas/mw/log/design/slot_drainer_action_diagram_design.uxf?ref=be7b21852077fc3db76686d352d9907ec8495a08)

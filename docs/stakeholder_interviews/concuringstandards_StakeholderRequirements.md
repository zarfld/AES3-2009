This is a sophisticated design challenge, as a hardware-agnostic AES3 stack must satisfy requirements from foundational audio standards (AES) and complex network synchronization and transport standards (IEEE 1722, 802.1AS).

The requirements imposed by these standards compel the software stack, particularly the Hardware Abstraction Layer (HAL), to perform critical data reformatting, time synchronization, and resource management functions that would typically be handled rigidly by dedicated serial hardware.

Here are the mandatory requirements imposed on your AES3 SW stack by concurrent and related standards:

---

## 1. Requirements Imposed by Concurrent Core AES Standards (Content & Timing)

The core AES standards define what the AES3 stream is and how clocks must behave in a professional environment, imposing strict conformance requirements on the audio processing layer:

### A. AES3 and AES5 (Content Definition)
The implementation must adhere to the semantic structure and sampling frequency rules defined by the AES standards:

*   The content layer of the AES3 SW stack must be decomposed into four parts corresponding to the standards: **Audio Content** (Part 1), **Metadata and Subcode** (Part 2), **Transport** (Part 3), and **Physical and Electrical** (Part 4).
*   The stack must use sampling frequencies that are in accordance with **AES5-2008** (and its revisions).
*   If the system carries non-linearly represented audio (e.g., coded audio), the **validity bit (V bit) shall be set to logic 1** to indicate that the audio sample word is *not suitable* for direct conversion to an analog signal.
*   The software must handle the **channel status bits** carrying information derived from the block, user data, and the use of auxiliary bits. An AES3 block consists of a sequence of **192 consecutive frames**, starting with a unique preamble type Z.

### B. AES11 (Synchronization Accuracy in Studio Operations)
When the software stack deals with synchronization (e.g., generating or receiving a Digital Audio Reference Signal, DARS), it must meet specific precision metrics derived from AES11:

*   The system must satisfy the timing and synchronization requirements necessary for time-sensitive applications.
*   The implementation must meet the time synchronization performance requirements corresponding to **Mask 2** for **professional digital audio applications**, a mask derived from AES3-2009 and AES11-2009.
*   For synchronized equipment, the timing-reference point difference between the DARS and all output signals, at the equipment connector points, **shall be less than $\pm$ 5%** (or $\pm$ 18$^\circ$) **of the AES3 frame period**.
*   Receivers must be designed to accept inputs where the difference between the DARS and the input signals is less than **$\pm$ 25% of the AES3 frame period** (or $\pm$ 90$^\circ$).

---

## 2. Requirements Imposed by Referencing Standards (Network Transport & Control)

For a hardware-agnostic design focused on modern networks (Time-Sensitive Networking, TSN/AVB), the IEEE standards impose complex requirements handled by the hardware abstraction layer (HAL) and the control layer.

### A. IEEE 1722 (AVTP Audio Format / AAF)

If your SW stack uses AVTP (Audio/Video Transport Protocol) for transport over Ethernet, it must conform to the **AVTP Audio Format (AAF)**, subtype **AES3\_32BIT**. This mandates a specific reformatting process within the HAL:

1.  **Data Alignment and Order (LSB-to-MSB Conversion):**
    *   The SW stack must manage the difference between the serial AES3 format and the packet-based AAF format.
    *   The bit order of the audio sample word within the AES3 subframe *shall* follow the **AVTP convention** (Most-Significant Bit, MSB, at the least offset), which is typical network byte order.
    *   All bits of the AES3 24-bit audio sample word field *shall* be transmitted without modification.

2.  **Subframe Reformatting (Preamble and Parity Handling):**
    *   AES3 subframes to be transmitted via AAF *must* be reformatted to match the AAF Subframe Format.
    *   The AES3 serial preambles (X, Y, Z) are **not transmitted**. Bits 0 to 3 in the AAF subframe (where the preamble bits used to be) *shall* be set to zero (0).
    *   The P (Parity) bit in the original AES3 subframe is replaced by the **B (Block Start) bit** in the AAF subframe format. The B bit *shall* be set to one (1) only for the first subframe of the AES3 block.

3.  **Data Type Communication:**
    *   The AAF header must explicitly reference the type of data carried. The `aes3_dt_ref` field *shall* be set to a value indicating the reference standard: **DT\_SMPTE338** (for professional applications) or **DT\_IEC61937** (for consumer applications).

### B. IEEE 802.1AS and 1588 (Precision Time Synchronization)

Since AES3 data transport is time-sensitive, the underlying network infrastructure is expected to be a **Time-Aware System** adhering to gPTP (Generalized Precision Time Protocol):

*   The implementation of a time-aware system *shall* support at least one IEEE 1588 PTP Instance.
*   The protocol *shall* ensure synchronization requirements are met for time-sensitive applications like audio and video across bridged networks, especially those using **IEEE 802.3 full duplex links**.
*   If synchronization requirements are met, any two PTP Instances separated by six or fewer hops (network latency) **will be synchronized to within 1 microsecond peak-to-peak** during steady-state operation.
*   The system must implement the **Best Master Clock Algorithm (BMCA)** to select the synchronized time source (Grandmaster).
*   The synchronization process requires that all clocks in a gPTP domain are logically **syntonized** (meaning they measure time intervals using the same frequency), and this process is mandatory in gPTP.
*   **Hardware Interface Requirement:** When the software accesses 1588 time sync registers that are split into low (xxxL) and high (xxxH) portions, it *should* access the **xxxL register first and only then the xxxH register** to prevent impact on hardware functionality.

### C. IEEE 1722.1 (ATDECC/AVDECC - Discovery and Control)

This standard governs the vendor-agnostic control and management interface of the SW stack:

*   The End Station *shall* implement at least one **ATDECC Entity** (formerly AVDECC).
*   If the stack acts as a Talker or Listener, it *shall* implement the corresponding ATDECC role state machines and use **ATDECC messages transported via IEEE 1722 AVTPDUs**.
*   The software must support **enumeration** (discovery of capabilities, formats, and controls). This is facilitated by descriptors like `AUDIO_CLUSTER` and the transmission of information through the **ATDECC Discovery Protocol (ADP)**.
*   The stack *must* support mechanisms for a Controller to manage access, using commands like **ACQUIRE\_ENTITY** and **LOCK\_ENTITY**.
*   The SW stack must be able to verify that its own Entity Model (AEM) has not been tampered with since creation using the **AUTH\_GET\_IDENTITY** command, which relies on cryptographic signatures.

### D. Security and Transport (IEEE 1722 and PCIe Standards)

The hardware-agnostic nature of the project means the underlying OS/HAL must respect fundamental security and bus communication standards:

*   **Encryption (AEF):** If stream encryption is supported, the system must use the **AES Encrypted Format (AEF)**, which relies on the **AES-SIV cryptographic algorithm**. Encryption can only be enabled on a stream that is **not currently streaming** (non-Active Stream).
*   **PCIe Compliance (HAL Requirement):** The underlying hardware interface software must adhere to PCIe requirements, ensuring that devices are **enumerable** by deployed PCI software and conform to **PCI ordering and cache coherency rules**.
*   **Resource Arbitration:** For access to shared resources (like Flash/EEPROM), software operations (other than BIOS) *must* **take a semaphore** before proceeding to avoid conflicts.

***

**In essence, these requirements define your AES3 SW stack as a translation engine:** it must translate the rigorous, low-level timing and data integrity demands of professional AES audio (defined by **AES3/AES11**) into the packet-centric, time-stamped framework of modern network protocols (**IEEE 1722/802.1AS**), all while managing hardware access through abstract, regulated interfaces (like **PCIe** and **ATDECC**).
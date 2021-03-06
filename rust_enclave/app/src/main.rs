// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License..

extern crate bincode;
extern crate serde;
extern crate sgx_types;
extern crate sgx_urts;

use serde::{Deserialize, Serialize};
use sgx_types::*;
use sgx_urts::SgxEnclave;

// Generated by bindgen frome enclave/my_struct.h
// Serialize and Deserialize are required by bincode/serde
#[repr(C)]
#[derive(Debug, Copy, Clone, Serialize, Deserialize)]
pub struct MyStruct {
    pub i: ::std::os::raw::c_int,
    pub f: f32,
}

static ENCLAVE_FILE: &'static str = "enclave.signed.so";

extern "C" {
    fn ecall_aes_gcm_128_encrypt(
        eid: sgx_enclave_id_t,
        retval: *mut sgx_status_t,
        plaintext: *const u8,
        text_len: usize,
        ciphertext: *mut u8,
        mac: &mut [u8; 16],
    ) -> sgx_status_t;

    fn ecall_aes_gcm_128_decrypt(
        eid: sgx_enclave_id_t,
        retval: *mut sgx_status_t,
        ciphertext: *const u8,
        text_len: usize,
        mac: &[u8; 16],
        plaintext: *mut u8,
    ) -> sgx_status_t;

    fn ecall_bindgen_send_datastructure(eid: sgx_enclave_id_t, s: MyStruct) -> sgx_status_t;

    fn ecall_bincode_send_datastructure(
        eid: sgx_enclave_id_t,
        serialized_data: *const u8,
        data_len: usize,
    ) -> sgx_status_t;
}

fn init_enclave() -> SgxResult<SgxEnclave> {
    let mut launch_token: sgx_launch_token_t = [0; 1024];
    let mut launch_token_updated: i32 = 0;
    // call sgx_create_enclave to initialize an enclave instance
    // Debug Support: set 2nd parameter to 1
    let debug = 1;
    let mut misc_attr = sgx_misc_attribute_t {
        secs_attr: sgx_attributes_t { flags: 0, xfrm: 0 },
        misc_select: 0,
    };
    SgxEnclave::create(
        ENCLAVE_FILE,
        debug,
        &mut launch_token,
        &mut launch_token_updated,
        &mut misc_attr,
    )
}

fn main() {
    let enclave = match init_enclave() {
        Ok(r) => {
            println!("[+] Init Enclave Successful {}!", r.geteid());
            r
        }
        Err(x) => {
            println!("[-] Init Enclave Failed {}!", x.as_str());
            return;
        }
    };

    // ========== Send raw pointer across enclave interface ==========

    let input_string = String::from("This is a secret message.");
    let mut encrypted_vec = vec![0u8; input_string.len()];
    let mut mac_array: [u8; SGX_AESGCM_MAC_SIZE] = [0; SGX_AESGCM_MAC_SIZE];
    let mut retval = sgx_status_t::SGX_SUCCESS;

    // Encrypt the message
    let result = unsafe {
        ecall_aes_gcm_128_encrypt(
            enclave.geteid(),
            &mut retval,
            input_string.as_ptr() as *const u8,
            input_string.len(),
            encrypted_vec.as_ptr() as *mut u8,
            &mut mac_array,
        )
    };
    match result {
        sgx_status_t::SGX_SUCCESS => match retval {
            sgx_status_t::SGX_SUCCESS => (),
            e => eprintln!("Encrypt failed: {:?}", e),
        },
        _ => {
            println!("[-] ECALL aes_gcm_128_encrypt Failed {}!", result.as_str());
            return;
        }
    }

    println!(
        "\"{}\" has been encrypted: {:?}",
        input_string, encrypted_vec
    );

    let mut decrypted_vec = vec![0u8; encrypted_vec.len()];

    // Decrypt the message
    let result = unsafe {
        ecall_aes_gcm_128_decrypt(
            enclave.geteid(),
            &mut retval,
            encrypted_vec.as_ptr() as *const u8,
            encrypted_vec.len(),
            &mac_array,
            decrypted_vec.as_ptr() as *mut u8,
        )
    };
    match result {
        sgx_status_t::SGX_SUCCESS => match retval {
            sgx_status_t::SGX_SUCCESS => (),
            e => eprintln!("Decrypt failed: {:?}", e),
        },
        _ => {
            println!("[-] ECALL aes_gcm_128_decrypt Failed {}!", result.as_str());
            return;
        }
    }

    let output_string = String::from_utf8(decrypted_vec.clone()).expect("Invalid UTF-8");
    println!(
        "{:?} has been decrypted: \"{}\"",
        encrypted_vec, output_string
    );

    // ========== Send C-like structure across enclave interface ==========

    let s = MyStruct {
        i: 7,
        f: std::f32::consts::PI,
    };

    let result = unsafe { ecall_bindgen_send_datastructure(enclave.geteid(), s) };
    match result {
        sgx_status_t::SGX_SUCCESS => (),
        _ => {
            println!(
                "[-] ECALL bindgen_send_datastructure Failed {}!",
                result.as_str()
            );
            return;
        }
    }

    // ========== Send serialized structure across enclave interface ==========

    let s = MyStruct {
        i: 777,
        f: std::f32::consts::E,
    };
    let serialized_data: Vec<u8> = bincode::serialize(&s).unwrap();

    let result = unsafe {
        ecall_bincode_send_datastructure(
            enclave.geteid(),
            serialized_data.as_ptr() as *const u8,
            serialized_data.len(),
        )
    };
    match result {
        sgx_status_t::SGX_SUCCESS => (),
        _ => {
            println!(
                "[-] ECALL bincode_send_datastructure Failed {}!",
                result.as_str()
            );
            return;
        }
    }

    println!("[+] rust_enclave success...");
    enclave.destroy();
}

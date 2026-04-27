//from myLib - ruststd

//hashes a String type as a unsigned 4 byte int
pub fn str(s: &String) -> u32 {
    let mut hash = 2166136261u32; // FNV offset basis
    
    for byte in s.bytes() {
        hash ^= byte as u32;
        hash = hash.wrapping_mul(16777619); // FNV prime
    }
    
    return hash;
}
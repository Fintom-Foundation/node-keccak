/// <reference types="node" />
export interface Keccak {
  /**
   * Update the Keccak Hash Content
   * @param input The Input String or Buffer
   */
  update(input?: string | Buffer): Keccak;
  /**
   * Digest the Keccak Hash Content
   * @param format The Hash Output Format (default as binary)
   */
  digest(format?: 'binary' | 'hex'): string;
}
export declare var Keccak: {
  /**
   * The Constructor for creating a byte array (default is Keccak-512)
   */
  new (bytes?: 224 | 256 | 384 | 512): Keccak;
};

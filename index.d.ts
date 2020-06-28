/// <reference types="node" />
export interface SHA3Hash {
  /**
   * Update the SHA3 Hash Content
   * @param input The Input String or Buffer
   */
  update(input?: string | Buffer): SHA3Hash;
  /**
   * Digest the SHA3 Hash Content
   * @param format The Hash Output Format (default as binary)
   */
  digest(format?: 'binary' | 'hex'): string;
}
export declare var SHA3Hash: {
  /**
   * The Constructor for creating a byte array (default is SHA3-512)
   */
  new (bytes?: 224 | 256 | 384 | 512): SHA3Hash;
};

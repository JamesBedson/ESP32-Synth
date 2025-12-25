import { defineConfig } from 'vite'

export default defineConfig({
  root: './',
  build: {
    outDir: '../data-build',  // Build to separate folder
    emptyOutDir: true,
    minify: 'terser',         // Better compression
    assetsInlineLimit: 0,     // Don't inline assets
  }
})
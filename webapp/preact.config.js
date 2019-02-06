export default (config, env, helpers) => {
    config.devServer = {
      quiet: false,
      proxy: [
        {
          path: '/api/**',
          target: 'http://localhost:3000/',
          // ...any other stuff...
        }
      ]
    }
  }
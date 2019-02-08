export default (config, env, helpers) => {
    config.devServer = {
      quiet: false,
      proxy: [
        {
          path: '/api/**',
          target: 'http://carpi/',
          // ...any other stuff...
        }
      ]
    }
  }
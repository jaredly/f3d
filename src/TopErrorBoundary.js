
const React = require('react')


class ErrorBoundary extends React.Component {
  constructor(props) {
    super(props);
    this.state = { error: null };
  }

  componentDidCatch(error, info) {
    this.setState({ error });
    Bugsnag.notifyException(error, { react: info })
  }

  render() {
    if (this.state.error) {
      // You can render any custom fallback UI
      return React.createElement("div", {
        style: {
          padding: '100px',
          fontSize: '24px',
        }
      },
      "Something broke. Sorry, we'll try to fix it. Try refreshing.",
      React.createElement("pre", {
        style: {
          backgroundColor: "#eee",
          fontSize: '14px',
          padding: '20px',
          overflow: 'auto',
        }
      },
      this.state.error.message,'\n\n',
      this.state.error.stack

    )
    );
    }
    return this.props.children;
  }
}

module.exports = ErrorBoundary
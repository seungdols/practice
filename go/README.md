vim-go에서 아래와 같은 오류가 발생할 경우가 있다. 

```bash
malformed import path "go: cannot find main module
```

```bash
go env -w GO111MODULE=auto
```

위의 명령어를 입력 해주면 정상적으로 해결 된다.

ref. https://go.dev/blog/go116-module-changes

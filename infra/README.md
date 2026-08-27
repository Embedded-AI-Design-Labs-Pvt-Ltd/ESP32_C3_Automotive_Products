# Infrastructure

Planned only in Phase 2. No live cloud resources are created until implementation is approved.

| Path | Purpose |
|---|---|
| `docker/` | IDF build image, host unit-test image, Virtual ECU compose |
| `jenkins/` | Lint, unit, component build matrix, artifact publish |
| `terraform/aws/` | Firmware artifact bucket, optional OTA staging |
| `terraform/gcp/` | Equivalent GCS + Cloud Build hook |
| `terraform/azure/` | Equivalent Blob + pipeline hook |

Prototype cloud is for **artifact storage and CI**, not vehicle backend production.

---

**Embedded AI Design Labs Pvt Ltd**  
Muhammad Samiullah  
CTO & Founder  
© 2026 Copyright. All rights reserved.

